pipeline {
  options {
    skipDefaultCheckout()
  }
  agent { label 'x86_64' }
  stages {
    stage ('checkout') {
      steps {
        script {
          checkout changelog: false, poll: false, scm: [$class: 'GitSCM', branches: [[name: '*/ci-integration-test']], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'PreBuildMerge', options: [fastForwardMode: 'FF', mergeRemote: 'origin', mergeStrategy: <object of type org.jenkinsci.plugins.gitclient.MergeCommand.Strategy>, mergeTarget: 'ci-integration-develop']]], submoduleCfg: [], userRemoteConfigs: [[credentialsId: 'sorabot-github-user', url: 'https://github.com/hyperledger/iroha.git']]]
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
