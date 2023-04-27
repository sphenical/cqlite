pipeline {

    agent {
        docker {
            label 'docker'
            image 'sphenic-build'
        }
    }

    parameters {
        booleanParameter (name: 'CLEAN', defaultValue: false, description: 'Force a clean build')
    }

    stages {

        stage ('Clean') {
            when { expression { return params.CLEAN } }
            steps {
                dir ('build') {
                    deleteDir ()
                }
                dir ('build-debug') {
                    deleteDir ()
                }
            }
        }
        stage ('Configure') {
            steps {
                sh '''
                    cmake -B build \
                        -DCMAKE_BUILD_TYPE=Release \
                        -DCQLITE_BUILD_TESTS=ON \
                        -DCQLITE_BUILD_DOCUMENTATION=ON \
                        .
                '''
                sh '''
                    cmake -B build-debug \
                       -DCMAKE_BUILD_TYPE=Debug \
                       -DCQLITE_BUILD_TESTS=ON \
                       -DCQLITE_BUILD_DOCUMENTATION=ON \
                       -DCMAKE_CXX_FLAGS="-W -Wall -Wextra --coverage" \
                       -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
                       .
                '''
            }
        }
        stage ('Build') {
            parallel {
                stage ('Build release') {
                    steps {
                        sh 'cmake --build build'
                    }
                }
                stage ('Build debug') {
                    steps {
                        sh 'cmake --build build-debug | tee compiler-output.txt'
                    }
                }
            }
        }
        stage ('Checks and docs') {
            parallel {
                stage ('Test') {
                    steps {
                        sh 'cmake --build build --target check'
                        dir ('build') {
                            junit '**/test-results/*.xml'
                        }
                    }
                }
                stage ('Test coverage') {
                    steps {
                        dir ('build-debug') {
                            dir ('doc/coverage') {
                                echo 'Created the directory for the coverage report.'
                            }

                            sh """
                                lcov --capture --initial --no-external --directory ${WORKSPACE} \
                                    --output-file coverage_base.info
                            """

                            sh 'cmake --build . --target check'

                            sh """
                                lcov --capture --no-external --directory ${WORKSPACE} \
                                    --output-file coverage.info
                                lcov --add-tracefile coverage_base.info \
                                    --add-tracefile coverage.info \
                                    --output-file coverage.info
                                lcov --remove coverage.info '*/tests/*' \
                                    --output-file coverage.info
                                lcov --remove coverage.info '*/CMakeFiles/*' \
                                    --output-file coverage.info

                                genhtml --legend -o doc/coverage coverage.info
                            """

                            publishHTML (target: [
                                allowMissing: true,
                                alwaysLinkToLastBuild: true,
                                keepAll: true,
                                reportDir: 'doc/coverage',
                                reportFiles: 'index.html',
                                reportName: 'Test coverage report'])
                        }
                    }
                }
                stage ('Documentation') {
                    steps {
                        sh 'cmake --build build --target doc'

                        dir ('build') {

                            publishHTML (target: [
                                allowMissing: false,
                                alwaysLinkToLastBuild: true,
                                keepAll: true,
                                reportDir: 'doc/html',
                                reportFiles: 'index.html',
                                reportName: 'Doxygen Documentation'])
                        }
                    }
                }
            }
        }
        stage ('Package') {
            steps {
                sh 'cmake --build build --target package'

                dir ('build') {
                    archiveArtifacts (
                        artifacts: '*.gz,*.sh,*pdf',
                        allowEmptyArchive: false,
                        fingerprint: true,
                        onlyIfSuccessful: true
                    )
                }
            }
        }
    }
}

