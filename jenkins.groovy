pipeline {

    agent any

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
                    sh 'make -j5' 
                }
            }
        }
        stage ('Test') {
            steps {
                dir ('build') {
                    sh 'make -j5 check'
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
                        keepAll: false,
                        reportDir: 'doc/html',
                        reportFiles: 'index.html',
                        reportName: 'Doxygen Documentation',
                        reportTitles: 'Documentation'])
                }
            }
        }
        stage ('Package') {
            steps {
                dir ('build') {
                    sh 'make -j5 package'
                    archive '*.gz,*.sh,*.xml,tests/Testing/**/*'
                }
            }
        }
    }
}

