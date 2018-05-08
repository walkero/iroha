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
              def approvalsRequired = 1
              def mergeApproval = input message: 'Your PR has been built successfully. Merge it now?', submitterParameter: 'jenkinsCommitterEmail',
                parameters: [booleanParam(defaultValue: false, description: '', name: "I confirm I want to merge ${CHANGE_BRANCH} into ${CHANGE_TARGET}")]
              if (mergeApproval) {
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
                    if (approvalsRequired > 0) {
                      sh "echo 'Merge failed. Get more PR approvals before merging'"
                      return false
                    }
                    return true
                  }
                }
              }
              else {
                currentBuild.result = 'ABORTED'
                error("Merge has been aborted by ${mergeApproval['jenkinsCommitterEmail']}")
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
