// Overall pipeline looks like the following
//
// |--Stop excess jobs--|----BUILD----|--Pre-Coverage--|------Test------|-Post Coverage-|-Build rest-|--Publish--|
//                      |             |                |                |               |
//                      |-> Linux     |-> lcov         |-> Linux        |-> lcov        |-> gen docs
//                      |-> ARMv7                      |-> ARMv7        |-> SonarQube   |-> bindings
//                      |-> ARMv8                      |-> ARMv8
//                      |-> MacOS                      |-> MacOS

// NOTE: In build stage we differentiate only platforms in pipeline scheme. Build/Release is filtered inside the platform
// TODO: release build in case branches master/develop (previous pipeline)
// TODO: limit stage of pipeline for execution: 3 hours
// (postponed) TODO: limit nightly build pipeline execution for 3 days max
// (pending) TODO: how to do all types of tests on all platforms (create stage in parallel for each platform and test - 4x5=20?!?!)
// TODO: upload artifacts at the post stage of each platform

properties([parameters([
  choice(choices: 'Debug\nRelease', description: '', name: 'BUILD_TYPE'),
  booleanParam(defaultValue: true, description: '', name: 'Linux'),
  booleanParam(defaultValue: false, description: '', name: 'ARMv7'),
  booleanParam(defaultValue: false, description: '', name: 'ARMv8'),
  booleanParam(defaultValue: false, description: '', name: 'MacOS'),
  booleanParam(defaultValue: false, description: 'Whether it is a triggered build', name: 'Nightly'),
  booleanParam(defaultValue: false, description: 'Whether build docs or not', name: 'Doxygen'),
  booleanParam(defaultValue: false, description: 'Whether build Java bindings', name: 'JavaBindings'),
  booleanParam(defaultValue: false, description: 'Whether build Python bindings', name: 'PythonBindings'),
  booleanParam(defaultValue: false, description: 'Whether build bindings only w/o Iroha itself', name: 'BindingsOnly'),
  string(defaultValue: '4', description: 'How much parallelism should we exploit. "4" is optimal for machines with modest amount of memory and at least 4 cores', name: 'PARALLELISM')])])

pipeline {
  environment {
    CCACHE_DIR = '/opt/.ccache'
    CCACHE_RELEASE_DIR = '/opt/.ccache-release'
    SORABOT_TOKEN = credentials('SORABOT_TOKEN')
    SONAR_TOKEN = credentials('SONAR_TOKEN')
    CODECOV_TOKEN = credentials('CODECOV_TOKEN')
    DOCKERHUB = credentials('DOCKERHUB')
    DOCKER_BASE_IMAGE_DEVELOP = 'hyperledger/iroha:develop'
    DOCKER_BASE_IMAGE_RELEASE = 'hyperledger/iroha:latest'
    JENKINS_DOCKER_IMAGE_DIR = '/tmp/docker'

    IROHA_NETWORK = "iroha-0${CHANGE_ID}-${GIT_COMMIT}-${BUILD_NUMBER}"
    IROHA_POSTGRES_HOST = "pg-0${CHANGE_ID}-${GIT_COMMIT}-${BUILD_NUMBER}"
    IROHA_POSTGRES_USER = "pguser${GIT_COMMIT}"
    IROHA_POSTGRES_PASSWORD = "${GIT_COMMIT}"
    IROHA_POSTGRES_PORT = 5432

    dockerAgentDockerImage = ''
    ws_path = ''
  }

  options {
    buildDiscarder(logRotator(numToKeepStr: '20'))
  }

  agent any
  stages {
    stage ('Stop excess jobs') {
      agent { label 'master' }
      steps {
        script {
          if (BRANCH_NAME != "develop") {
            if (params.Nightly) {
                // Stop this job running if it is nightly but not the develop it should be
                // def tmp = load ".jenkinsci/cancel-nightly-except-develop.groovy"
                // tmp.cancelThisJob()
            }
            else {
              // Stop same job running builds if it is commit/PR build and not triggered as nightly
              def builds = load ".jenkinsci/cancel-builds-same-job.groovy"
              builds.cancelSameJobBuilds()
            }
          }
          else {
            if (!params.Nightly) {
              // Stop same job running builds if it is develop but it is not nightly
              def builds = load ".jenkinsci/cancel-builds-same-job.groovy"
              builds.cancelSameJobBuilds()
            }
          }
        }
      }
    }
    stage('Build') {
      parallel {
        stage('Linux') {
          when { expression { return params.Linux } }
          agent { label 'x86_64_aws_build' }
          steps {
            script {
              if (params.BUILD_TYPE == 'Debug') {
                debugBuild = load ".jenkinsci/debug-steps.groovy"
                coverage = load ".jenkinsci/selected-branches-coverage.groovy"
                if (coverage.selectedBranchesCoverage(['develop', 'master'])) {
                  debugBuild.doDebugBuild(true)
                }
                else {
                  debugBuild.doDebugBuild()
                }
              }
              else {
                releaseBuild = load ".jenkinsci/release-steps.groovy"
                releaseBuild.doReleaseBuild()
              }
            }
          }
          post {
            success {
              script {
              //TODO: develop handling successful build for x86_64
                sh 'echo build was successful'
              }
            }
            always {
              script {
                post = load ".jenkinsci/linux-post-step.groovy"
                post.linuxPostStep()
              }
            }
          }
        }
        stage('ARMv7') {
          when { expression { return params.ARMv7 } }
          agent { label 'armv7' }
          steps {
            script {
              if (params.BUILD_TYPE == 'Debug') {
                debugBuild = load ".jenkinsci/debug-steps.groovy"
                coverage = load ".jenkinsci/selected-branches-coverage.groovy"
                if (!params.Linux && !params.ARMv8 && !params.MacOS && 
                   (coverage.selectedBranchesCoverage(['develop', 'master']))) {
                  debugBuild.doDebugBuild(true)
                }              
                else {
                  debugBuild.doDebugBuild()
                }
              }
              else {
                releaseBuild = load ".jenkinsci/release-steps.groovy"
                releaseBuild.doReleaseBuild()
              }
            }
          }
          post {
            always {
              script {
                post = load ".jenkinsci/linux-post-step.groovy"
                post.linuxPostStep()
              }
            }
          }
        }
        stage('ARMv8') {
          when { expression { return params.ARMv8 } }
          agent { label 'armv8' }
          steps {
            script {
              if (params.BUILD_TYPE == 'Debug') {
                debugBuild = load ".jenkinsci/debug-steps.groovy"
                coverage = load ".jenkinsci/selected-branches-coverage.groovy"
                if (!params.Linux && !params.MacOS && 
                   (coverage.selectedBranchesCoverage(['develop', 'master']))) {
                  debugBuild.doDebugBuild(true)
                }
                else {
                  debugBuild.doDebugBuild()
                }
              }
              else {
                releaseBuild = load ".jenkinsci/release-steps.groovy"
                releaseBuild.doReleaseBuild()
              }
            }
          }
          post {
            always {
              script {
                post = load ".jenkinsci/linux-post-step.groovy"
                post.linuxPostStep()
              }
            }
          }
        }
        stage('MacOS') {
          when { expression { return params.MacOS } }
          agent { label 'mac' }
          steps {
            script {
              if (params.BUILD_TYPE == 'Debug') {
                debugBuild = load ".jenkinsci/mac-debug-steps.groovy"
                coverage = load ".jenkinsci/selected-branches-coverage.groovy"
                if (!params.Linux && 
                   (coverage.selectedBranchesCoverage(['develop', 'master']))) {
                  debugBuild.doDebugBuild(true)
                }
                else {
                  debugBuild.doDebugBuild() 
                }
              }
              else {
                releaseBuild = load ".jenkinsci/mac-release-steps.groovy"
                releaseBuild.doReleaseBuild()
              }
            }
          }
          post {
            always {
              script {
                post = load ".jenkinsci/linux-post-step.groovy"
                post.linuxPostStep()
              }
            }
          }
        }
      }
    }
    // TODO: add cause whether run this step or not 
    stage('Pre-Coverage') {
      when {
        allOf {
          expression { return ! params.BindingsOnly }
          //expression { return params.
        }
      }
      agent { label 'x86_64_aws_cov' }
      steps {
        script {
          if (params.BUILD_TYPE == 'Debug') {
            debugBuild = load ".jenkinsci/debug-steps.groovy"
            debugBuild.doPreCoverageStep()
          }
        }
      }
    }
    stage('Tests') {
      when {
        allOf {
          expression { return ! params.BindingsOnly }
        }
      }
      parallel {
        stage('Linux') {
          when { expression { return params.Linux } }
          agent { label 'x86_64_aws_test' }
          steps {
            script {
              tests = load ".jenkinsci/debug-steps.groovy"
              tests.doTestStep()  //TODO: pass tests list 
            }
          }
        }
        stage('ARMv7') {
          when { expression { return params.ARMv7 } }
          agent { label 'armv7' }
          steps {
            script {
              tests = load ".jenkinsci/debug-steps.groovy"
              tests.doTestStep()  //TODO: pass tests list 
            }
          }
        }
        stage('ARMv8') {
          when { expression { return params.ARMv8 } }
          agent { label 'armv8' }
          steps {
            script {
              tests = load ".jenkinsci/debug-steps.groovy"
              tests.doTestStep()  //TODO: pass tests list 
            }
          }
        }
        stage('MacOS') {
          when { expression { return params.MacOS } }
          agent { label 'mac' }
          steps {
            script {
              tests = load ".jenkinsci/mac-debug-steps.groovy"
              tests.doTestStep()  //TODO: pass tests list 
            }
          }
        }
      }
    }
    // TODO: add cause whether run this step or not 
    stage('Post-Coverage') {
      when {
        allOf {
          expression { return ! params.BindingsOnly }
        }
      }
      parallel {
        stage('lcov_cobertura') {
          // when { expression { return params.MacOS } }
          agent { label 'x86_64_aws_cov' }
          steps {
            script {
              if (params.BUILD_TYPE == 'Debug') {
                debugBuild = load ".jenkinsci/debug-steps.groovy"
                debugBuild.doPostCoverageCoberturaStep()
              }
            }
          }
        }
        stage('sonarqube') {
          // when { expression { return params.MacOS } }
          agent { label 'armv7' }
          steps {
            script {
              if (params.BUILD_TYPE == 'Debug') {
                debugBuild = load ".jenkinsci/debug-steps.groovy"
                debugBuild.doPostCoverageSonarStep()
              }
            }
          }
        }
      }
    }
    stage('Build rest') {
      parallel {
        stage('docs') {
          when {
          allOf {
              expression { return params.Doxygen }
              expression { BRANCH_NAME ==~ /(master|develop)/ }
            }
          }
          // build docs on any vacant node. Prefer `x86_64` over
          // others as nodes are more powerful
          agent { label 'x86_64_aws_cov || arm' }
          steps {
            script {
              def doxygen = load ".jenkinsci/doxygen.groovy"
              docker.image("${env.DOCKER_IMAGE}").inside {
                def scmVars = checkout scm
                doxygen.doDoxygen()
              }
            }
          }
        }
        stage('bindings') {
          when {
            anyOf {
              expression { return params.BindingsOnly }
              expression { return params.PythonBindings }
              expression { return params.JavaBindings }
            }
          }
          agent { label 'x86_64_aws_cov' }
          steps {
            script {
              def bindings = load ".jenkinsci/bindings.groovy"
              def platform = sh(script: 'uname -m', returnStdout: true).trim()
              sh "curl -L -o /tmp/${env.GIT_COMMIT}/Dockerfile --create-dirs https://raw.githubusercontent.com/hyperledger/iroha/${env.GIT_COMMIT}/docker/develop/${platform}/Dockerfile"
              iC = docker.build("hyperledger/iroha-develop:${GIT_COMMIT}-${BUILD_NUMBER}", "-f /tmp/${env.GIT_COMMIT}/Dockerfile /tmp/${env.GIT_COMMIT} --build-arg PARALLELISM=${PARALLELISM}")
              sh "rm -rf /tmp/${env.GIT_COMMIT}"
              iC.inside {
                def scmVars = checkout scm
                bindings.doBindings()
              }
            }
          }
        }
      }
    }
    stage('Publish') {
      steps {
        script {
            //TODO: finish publish phase or get rid of it
            sh 'echo 123'
        }
      }
    }
  }
}