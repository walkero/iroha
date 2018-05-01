pipeline {
  options {
    skipDefaultCheckout()
  }
  agent { label 'x86_64' }
  stages {
    stage ('checkout') {
      steps {
        script {
          if (CHANGE_ID) {
            sh "echo this is a PR"
            checkout changelog: false, poll: false, scm: [$class: 'GitSCM', branches: 
            [[name: "${BRANCH_NAME}"]], doGenerateSubmoduleConfigurations: false, extensions: 
            [[$class: 'PreBuildMerge', options: [fastForwardMode: 'NO_FF', mergeRemote: 'origin', 
            mergeStrategy: 'default', mergeTarget: 'ci-integration-develop']], [$class: 'LocalBranch'], 
            [$class: 'CleanCheckout'], [$class: 'PruneStaleBranch'], [$class: 'UserIdentity', 
            email: 'jenkins@soramitsu.co.jp', name: 'jenkins']], submoduleCfg: [], userRemoteConfigs: 
            [[credentialsId: 'sorabot-github-user', url: 'https://github.com/hyperledger/iroha.git']]]
          }
          else {
            sh "echo this is NOT a PR"
            checkout changelog: false, poll: false, scm: [$class: 'GitSCM', branches: 
            [[name: "${BRANCH_NAME}"]], doGenerateSubmoduleConfigurations: false, extensions: 
            [[$class: 'LocalBranch'], [$class: 'UserIdentity', email: 'jenkins@soramitsu.co.jp', 
            name: 'jenkins']], submoduleCfg: [], userRemoteConfigs: [[credentialsId: 'sorabot-github-user', 
            url: 'https://github.com/hyperledger/iroha.git']]]
          }
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
