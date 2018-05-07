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
          [[name: "${CHANGE_BRANCH}"]], doGenerateSubmoduleConfigurations: false, extensions: 
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
              parameters: [choice(name: 'Merge?', choices: 'no\nyes', description: 'Choose "yes" if you want to merge this PR with develop branch')]
            if (env.READY_TO_MERGE == 'yes') {
              def committerEmail = sh(
                script: 'git --no-pager show -s --format=\'%ae\'', returnStdout: true).trim()
              withCredentials([string(credentialsId: 'jenkins-integration-test', variable: 'sorabot')]) {
                if (env.CHANGE_ID) {
                  def slurper = new groovy.json.JsonSlurperClassic()
                  sh """
                    echo User email from commit: ${committerEmail}
                    echo User email from plugin: ${BUILD_USER_EMAIL}
                  """
                  def jsonResponse = sh(script: """
                    curl -H "Authorization: token ${sorabot}" -H "Accept: application/vnd.github.v3+json" https://api.github.com/repos/hyperledger/iroha/pulls/${CHANGE_ID}/reviews
                  """, returnStdout: true).trim()
                  jsonResponse = slurper.parseText(jsonResponse)
                  sh """
                    echo "${jsonResponse.size()}"
                  """
                  if (jsonResponse.size() > 0) {
                    jsonResponse.each {
                      sh "echo state is: ${it.state}"
                    }
                  }
                  // sh("git push https://${sorabot}@github.com/hyperledger/iroha.git HEAD:ci-integration-develop")
                }
              }
            }
          }
        }
      }
    }
  }
}
