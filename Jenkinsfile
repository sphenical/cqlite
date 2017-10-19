#!/usr/bin/env groovy

pipeline {

    agent any
    
    parameters {
        string (
            name: 'BranchName',
            defaultValue: 'refs/heads/master',
            description: 'The branch or commit to build')
    }

    stages {

        stage ('Fetch') {
            steps {
                checkout ([
                    $class: 'GitSCM',
                    userRemoteConfigs: [[url: 'https://github.com/sphenical/cqlite.git']],
                    branches: [[name: "${params.BranchName}"]]
                ])
            }
        }
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

