pipeline {
  options {
    skipDefaultCheckout()
  }
  agent { label 'x86_64' }
  stages {
    stage ('checkout') {
      steps {
        script {
          checkout changelog: false, poll: false, scm: [$class: 'GitSCM', branches: [[name: "${env.GIT_BRANCH}"]], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'PreBuildMerge', options: [fastForwardMode: 'NO_FF', mergeRemote: 'origin', mergeStrategy: 'default', mergeTarget: 'ci-integration-develop']], [$class: 'LocalBranch']], submoduleCfg: [], userRemoteConfigs: [[credentialsId: 'sorabot-github-user', url: 'https://github.com/hyperledger/iroha.git']]]
        }
      }
    }
  }
  post {
    always {
      script {
        cleanWs()
      }
    }
  }
}
