pipeline {
  options {
    skipDefaultCheckout()
  }
  agent { label 'linux && x86_64' }
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
            def jenkinsCommiterEmail = ''
            def approvalsRequired = 2
            env.READY_TO_MERGE = input message: 'Your PR has been built successfully. Merge it now?',
              parameters: [choice(name: 'Merge?', choices: 'no\nyes', description: 'Choose "yes" if you want to merge this PR with develop branch')]
            if (env.READY_TO_MERGE == 'yes') {
              def gitCommitterEmail = sh(
                script: 'git --no-pager show -s --format=\'%ae\'', returnStdout: true).trim()
              wrap([$class: 'BuildUser']) {
                jenkinsCommiterEmail = env.BUILD_USER_EMAIL
              }
              withCredentials([string(credentialsId: 'jenkins-integration-test', variable: 'sorabot')]) {
                if (env.CHANGE_ID) {
                  def slurper = new groovy.json.JsonSlurperClassic()
                  def jsonResponseReview = sh(script: """
                    curl -H "Authorization: token ${sorabot}" -H "Accept: application/vnd.github.v3+json" https://api.github.com/repos/hyperledger/iroha/pulls/${CHANGE_ID}/reviews
                  """, returnStdout: true).trim()
                  jsonResponseReview = slurper.parseText(jsonResponseReview)
                  if (jsonResponseReview.size() > 0) {
                    jsonResponseReview.each {
                      if ("${it.state}" == "APPROVED") {
                        approvalsRequired -= 1
                      }
                    }
                  }
                  sh "echo approvals: ${approvalsRequired}"
                  if (approvalsRequired <= 0 && gitCommitterEmail == jenkinsCommiterEmail) {
                    echo 'This commit is mergeable'
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
}
