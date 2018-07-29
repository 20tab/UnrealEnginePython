def scmVars = [:]

pipeline {
    agent any

    options {
        skipDefaultCheckout()
    }

    environment {
        GCP_PROJECT_ID = sh(script: "gcloud config get-value project", returnStdout: true).trim()
        SLACK_CHANNEL = "synth-automation"
    }

    stages {
        stage("Checkout") {
            steps {
                script {
                    scmVars = checkout(scm)

                    scmVars.GIT_URL = scmVars.GIT_URL.replaceFirst(/\.git$/, "")
                    scmVars.GIT_REPOSITORY = scmVars.GIT_URL.replaceFirst(/^[a-z]+:\/\/[^\/]*\//, "")
                    scmVars.GIT_AUTHOR = sh(script: "${GIT_EXEC_PATH}/git log -1 --pretty=%an ${scmVars.GIT_COMMIT}", returnStdout: true).trim()
                    scmVars.GIT_MESSAGE = sh(script: "${GIT_EXEC_PATH}/git log -1 --pretty=%B ${scmVars.GIT_COMMIT}", returnStdout: true).trim()

                    scmVars.each { k, v ->
                         env."${k}" = "${v}"
                    }
                }
            }
        }

        stage("Trigger") {
            steps {
                script {
                    withCredentials([[$class: "UsernamePasswordMultiBinding", credentialsId: "jenkins-api",
                                      usernameVariable: "JENKINS_USERNAME", passwordVariable: "JENKINS_TOKEN"]]) {
                        if (scmVars.GIT_BRANCH == "master") {
                            sh('/opt/bitnami/common/bin/curl -k -X POST -u "${JENKINS_USERNAME}:${JENKINS_TOKEN}" "${JENKINS_URL}job/ue4-docker/job/master/build"')
                        }
                    }
                }
            }
        }
    }

    post {
        success {
            slackSend(channel: env.SLACK_CHANNEL,
                      color: "good",
                      message: "Success: ${scmVars.GIT_AUTHOR.split()[0]}'s build <${currentBuild.absoluteUrl}|${currentBuild.displayName}> in <${scmVars.GIT_URL}|${scmVars.GIT_REPOSITORY}> (<${scmVars.GIT_URL}/commit/${scmVars.GIT_COMMIT}|${scmVars.GIT_COMMIT.substring(0,8)}> on <${scmVars.GIT_URL}/tree/${scmVars.GIT_BRANCH}|${scmVars.GIT_BRANCH}>)\n- ${scmVars.GIT_MESSAGE}")
        }

        failure {
            slackSend(channel: env.SLACK_CHANNEL,
                      color: "danger",
                      message: "Failure: ${scmVars.GIT_AUTHOR.split()[0]}'s build <${currentBuild.absoluteUrl}|${currentBuild.displayName}> in <${scmVars.GIT_URL}|${scmVars.GIT_REPOSITORY}> (<${scmVars.GIT_URL}/commit/${scmVars.GIT_COMMIT}|${scmVars.GIT_COMMIT.substring(0,8)}> on <${scmVars.GIT_URL}/tree/${scmVars.GIT_BRANCH}|${scmVars.GIT_BRANCH}>)\n- ${scmVars.GIT_MESSAGE}")
        }
    }
}
