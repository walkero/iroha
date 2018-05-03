pipeline {
  options {
    skipDefaultCheckout()
  }
  agent { label 'x86_64' }
  stages {
    stage ('checkout') {
      steps {
        script {
          checkout changelog: false, poll: false, scm: [$class: 'GitSCM', branches: 
          [[name: "${BRANCH_NAME}"]], doGenerateSubmoduleConfigurations: false, extensions: 
          [[$class: 'PreBuildMerge', options: [fastForwardMode: 'FF', mergeRemote: 'origin', 
          mergeStrategy: 'default', mergeTarget: 'ci-integration-develop']], [$class: 'LocalBranch'], 
          [$class: 'CleanCheckout'], [$class: 'PruneStaleBranch'], [$class: 'UserIdentity', 
          email: 'jenkins@soramitsu.co.jp', name: 'jenkins']], submoduleCfg: [], userRemoteConfigs: 
          [[credentialsId: 'sorabot-github-user', url: 'https://github.com/hyperledger/iroha.git']]]
        }
      }
      post {
        success {
          script {
            withCredentials([string(credentialsId: 'jenkins-integration-test', variable: 'sorabot')]) {
              sh("git push https://${sorabot}@github.com/hyperledger/iroha.git HEAD:ci-integration-develop")
            }
          }
        }
      }
    }
  }
}
