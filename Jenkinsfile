pipeline {
  options {
    skipDefaultCheckout()
  }
  agent { label 'x86_64' }
  stages {
    stage ('checkout') {
      steps {
        script {
          checkout([$class: 'GitSCM', branches: [[name: '*/ready/**']], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'CleanBeforeCheckout'], pretestedIntegration(gitIntegrationStrategy: squash(), integrationBranch: 'ci-integration-develop', repoName: 'origin')], submoduleCfg: [], userRemoteConfigs: [[credentialsId: 'sorabot-github-user', url: 'https://github.com/hyperledger/iroha.git']]])
        }
      }
    }
    stage('publish') {
      steps {
        pretestedIntegrationPublisher()
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
