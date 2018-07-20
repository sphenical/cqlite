pipeline {

    agent any

    environment {
        NR_JOBS = sh (
                script: 'echo $((`nproc` + 1))',
                returnStdout: true)
            .trim ()
    }

    stages {

        stage ('Clean') { 
            steps {
                dir ('build') {
                    deleteDir ()
                }
            }
        }
        stage ('Prepare') { 
            steps {
                dir ('build') {
                    sh 'cmake -DCMAKE_BUILD_TYPE=Release -DCQLITE_BUILD_TESTS=ON -DCQLITE_BUILD_DOCUMENTATION=ON ..'
                }
            }
        }
        stage ('Build') { 
            steps {
                dir ('build') {
                    sh "make -j${NR_JOBS}"
                }
            }
        }
        stage ('Test') {
            steps {
                dir ('build') {
                    sh "make -j${NR_JOBS} check"
                    junit 'cqlite_testresults.xml' 
                }
            }
        }
        stage ('Documentation') {
            steps {
                dir ('build') {
                    sh 'make doc'

                    publishHTML (target: [
                        allowMissing: false,
                        alwaysLinkToLastBuild: false,
                        keepAll: true,
                        reportDir: 'doc/html',
                        reportFiles: 'index.html',
                        reportName: 'Doxygen Documentation'])
                }
            }
        }
        stage ('Package') {
            steps {
                dir ('build') {
                    sh "make -j${NR_JOBS} package"
                    archive '*.gz,*.sh,*.xml,tests/Testing/**/*'
                }
            }
        }
    }
}

