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
            env.READY_TO_MERGE = input message: 'Your PR has been built successfully. Merge it now?',
              parameters: [choice(name: 'Merge?', choices: 'no\nyes', description: 'Choose "yes" if you want 
                to merge this PR with develop branch')]
            if (env.READY_TO_MERGE == 'yes') {
              withCredentials([string(credentialsId: 'jenkins-integration-test', variable: 'sorabot')]) {
                sh("git push https://${sorabot}@github.com/hyperledger/iroha.git HEAD:ci-integration-develop")
              }
            }
          }
        }
      }
    }
  }
}
