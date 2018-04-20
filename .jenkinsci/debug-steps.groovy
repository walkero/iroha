#!/usr/bin/env groovy
// TODO: create Amazon EFS and mount it to /var/lib/docker/images - check in pipeline
// TODO: docker image clear after not usable

def doDebugBuild(coverageEnabled=false) {
  def parallelism = params.PARALLELISM
  // params are always null unless job is started
  // this is the case for the FIRST build only.
  // So just set this to same value as default. 
  // This is a known bug. See https://issues.jenkins-ci.org/browse/JENKINS-41929
  if (parallelism == null) {
    parallelism = 4
  }
  if ("arm7" in env.NODE_NAME) {
    parallelism = 1
  }

  def platform = sh(script: 'uname -m', returnStdout: true).trim()
  sh "curl -L -o /tmp/${env.GIT_COMMIT}/Dockerfile --create-dirs https://raw.githubusercontent.com/hyperledger/iroha/${env.GIT_COMMIT}/docker/develop/${platform}/Dockerfile"
  // pull docker image in case we don't have one
  // speeds up consequent image builds as we simply tag them
  sh "docker pull ${DOCKER_BASE_IMAGE_DEVELOP}"
  // TODO: check if workspace_path is saved and used later on
  dockerImageFile = sh(script: "echo ${BRANCH_NAME} | cut -c 1-8", returnStdout: true).trim()
  def dPullOrBuild = load '.jenkinsci/docker-pull-or-build.groovy'
  def iC = dPullOrBuild.dockerPullOrUpdate()
  dockerAgentDockerImage = iC.imageName()

  if ( env.NODE_NAME ==~ /^x86_64.+/ ) {
      sh "docker save -o ${env.JENKINS_DOCKER_IMAGE_DIR}/${dockerImageFile} ${dockerAgentDockerImage}"
  }
  iC.inside(""
    + " -e IROHA_POSTGRES_HOST=${env.IROHA_POSTGRES_HOST}"
    + " -e IROHA_POSTGRES_PORT=${env.IROHA_POSTGRES_PORT}"
    + " -e IROHA_POSTGRES_USER=${env.IROHA_POSTGRES_USER}"
    + " -e IROHA_POSTGRES_PASSWORD=${env.IROHA_POSTGRES_PASSWORD}"
    + " -v ${CCACHE_DIR}:${CCACHE_DIR}") {

    def scmVars = checkout scm
    def cmakeOptions = ""
    if ( coverageEnabled ) {
      cmakeOptions = " -DCOVERAGE=ON "
    }
    env.IROHA_VERSION = "0x${scmVars.GIT_COMMIT}"
    env.IROHA_HOME = "/opt/iroha"
    env.IROHA_BUILD = "${env.IROHA_HOME}/build"

    sh """
      ccache --version
      ccache --show-stats
      ccache --zero-stats
      ccache --max-size=5G
    """  
    sh """
      cmake \
        -DTESTING=ON \
        -H. \
        -Bbuild \
        -DCMAKE_BUILD_TYPE=Debug \
        -DIROHA_VERSION=${env.IROHA_VERSION} \
        ${cmakeOptions}
    """
    sh "cmake --build build -- -j${parallelism}"
    sh "ccache --show-stats"
    sh "pwd; ls"
  }
}


def doPreCoverageStep() {
  if ( env.NODE_NAME ==~ /^x86_64.+/ ) {
    sh "docker load -i ${JENKINS_DOCKER_IMAGE_DIR}/${dockerImageFile}"
  }
  def iC = docker.image("${dockerAgentDockerImage}")
  iC.inside(""
    + " -v ${CCACHE_DIR}:${CCACHE_DIR}") {
      
      sh "cmake --build build --target coverage.init.info"
  }
}

// TODO: add tests list argument to pass to this function
// now there are only unit tests running      
def doTestStep() {
  sh "docker network create ${env.IROHA_NETWORK}"
  sh "pwd; ls"
  // docker.image('postgres:9.5').run(""
  //   + " -e POSTGRES_USER=${env.IROHA_POSTGRES_USER}"
  //   + " -e POSTGRES_PASSWORD=${env.IROHA_POSTGRES_PASSWORD}"
  //   + " --name ${env.IROHA_POSTGRES_HOST}"
  //   + " --network=${env.IROHA_NETWORK}")
  
  if ( env.NODE_NAME ==~ /^x86_64.+/ ) {
    sh "docker load -i ${env.JENKINS_DOCKER_IMAGE_DIR}/${dockerImageFile}"
  }
  // def iC = docker.image("${dockerAgentDockerImage}")
  // def path = sh(script: 'pwd', returnStdout: true);
  // iC.inside(""
  //   + " -e IROHA_POSTGRES_HOST=${env.IROHA_POSTGRES_HOST}"
  //   + " -e IROHA_POSTGRES_PORT=${env.IROHA_POSTGRES_PORT}"
  //   + " -e IROHA_POSTGRES_USER=${env.IROHA_POSTGRES_USER}"
  //   + " -e IROHA_POSTGRES_PASSWORD=${env.IROHA_POSTGRES_PASSWORD}"
  //   + " --network=${env.IROHA_NETWORK}"
  //   + " -v ${CCACHE_DIR}:${CCACHE_DIR}") {

  //     def testExitCode = sh(script: 'cmake --build build --target test', returnStatus: true)
  //     if (testExitCode != 0) {
  //       currentBuild.result = "UNSTABLE"
  //     }
  //     return
  // }
}


def doPostCoverageCoberturaStep() {
  if ( env.NODE_NAME ==~ /^x86_64.+/ ) {
    sh "docker load -i ${env.JENKINS_DOCKER_IMAGE_DIR}/${dockerImageFile}"
  }
  def iC = docker.image("${dockerAgentDockerImage}")
  // iC.inside(""
  //   // + " -e IROHA_POSTGRES_HOST=${env.IROHA_POSTGRES_HOST}"
  //   // + " -e IROHA_POSTGRES_PORT=${env.IROHA_POSTGRES_PORT}"
  //   // + " -e IROHA_POSTGRES_USER=${env.IROHA_POSTGRES_USER}"
  //   // + " -e IROHA_POSTGRES_PASSWORD=${env.IROHA_POSTGRES_PASSWORD}"
  //   + " -v ${CCACHE_DIR}:${CCACHE_DIR}") {

  //       sh "cmake --build build --target coverage.info"
  //       sh "python /tmp/lcov_cobertura.py build/reports/coverage.info -o build/reports/coverage.xml"
  //       cobertura autoUpdateHealth: false, autoUpdateStability: false, coberturaReportFile: '**/build/reports/coverage.xml', conditionalCoverageTargets: '75, 50, 0', failUnhealthy: false, failUnstable: false, lineCoverageTargets: '75, 50, 0', maxNumberOfBuilds: 50, methodCoverageTargets: '75, 50, 0', onlyStable: false, zoomCoverageChart: false
  //   }
  //   // TODO: if we need to do copy built binaries to the volume each time
  //   sh "cp ./build/bin/* /tmp/${GIT_COMMIT}/"
}


def doPostCoverageSonarStep() {
  if ( env.NODE_NAME ==~ /^x86_64.+/ ) {
    sh "docker load -i ${env.JENKINS_DOCKER_IMAGE_DIR}/${dockerImageFile}"
  }
  def iC = docker.image("${dockerAgentDockerImage}")
  // iC.inside(""
  //   // + " -e IROHA_POSTGRES_HOST=${env.IROHA_POSTGRES_HOST}"
  //   // + " -e IROHA_POSTGRES_PORT=${env.IROHA_POSTGRES_PORT}"
  //   // + " -e IROHA_POSTGRES_USER=${env.IROHA_POSTGRES_USER}"
  //   // + " -e IROHA_POSTGRES_PASSWORD=${env.IROHA_POSTGRES_PASSWORD}"
  //   + " -v ${CCACHE_DIR}:${CCACHE_DIR}") {

  //     sh "cmake --build build --target cppcheck"
  //     // Sonar
  //     if (env.CHANGE_ID != null) {
  //       sh """
  //         sonar-scanner \
  //           -Dsonar.github.disableInlineComments \
  //           -Dsonar.github.repository='hyperledger/iroha' \
  //           -Dsonar.analysis.mode=preview \
  //           -Dsonar.login=${SONAR_TOKEN} \
  //           -Dsonar.projectVersion=${BUILD_TAG} \
  //           -Dsonar.github.oauth=${SORABOT_TOKEN} \
  //           -Dsonar.github.pullRequest=${CHANGE_ID}
  //       """
  //     }
  // }
}
return this
