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
          mergeStrategy: 'default', mergeTarget: "${CHANGE_TARGET}"]], [$class: 'LocalBranch'], 
          [$class: 'CleanCheckout'], [$class: 'PruneStaleBranch'], [$class: 'UserIdentity', 
          email: 'jenkins@soramitsu.co.jp', name: 'jenkins']], submoduleCfg: [], userRemoteConfigs: 
          [[credentialsId: 'sorabot-github-user', url: 'https://github.com/hyperledger/iroha.git']]]
        }
      }
      post {
        success {
          waitUntil {
            script {
              def jenkinsCommitterEmail = ''
              def jenkinsUser = ''
              def approvalsRequired = 1
              env.READY_TO_MERGE = input message: 'Your PR has been built successfully. Merge it now?',
                parameters: [choice(name: 'Merge?', choices: 'no\nyes', description: "Choose 'yes' if you want to merge ${CHANGE_BRANCH} into ${CHANGE_TARGET}")]
              if (env.READY_TO_MERGE == 'yes') {
                def gitCommitterEmail = sh(
                  script: 'git --no-pager show -s --format=\'%ae\'', returnStdout: true).trim()
                wrap([$class: 'BuildUser']) {
                  jenkinsCommitterEmail = env.BUILD_USER_EMAIL
                  jenkinsUser = env.BUILD_USER
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
                    if (approvalsRequired > 0) {
                      sh "echo 'Merge failed. Get more PR approvals before merging'"
                      return false
                    }
                    elif (gitCommitterEmail != jenkinsCommitterEmail) {
                      sh "echo 'Merge failed. Email of the commit does not match Jenkins user'"
                      return false
                    }
                    return true
                  }
                }
              }
              else {
                currentBuild.result = 'ABORTED'
                error("Merge has been aborted by ${jenkinsUser}")
                return true
              }
            }
          }
          sh('echo This commit is mergeable')
          // sh("git push https://${sorabot}@github.com/hyperledger/iroha.git HEAD:ci-integration-develop")
        }
      }
    }
  }
}
