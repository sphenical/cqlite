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
                dir ('build-debug') {
                    deleteDir ()
                }
            }
        }
        stage ('Prepare') { 
            steps {
                dir ('build') {
                    sh 'cmake -DCMAKE_BUILD_TYPE=Release -DCQLITE_BUILD_TESTS=ON -DCQLITE_BUILD_DOCUMENTATION=ON ..'
                }
                dir ('build-debug') {
                    sh 'cmake -DCMAKE_BUILD_TYPE=Debug -DCQLITE_BUILD_TESTS=ON -DCQLITE_BUILD_DOCUMENTATION=ON ' +
                       '-DCMAKE_CXX_FLAGS="-W -Wall -Wextra --coverage" -DCMAKE_EXE_LINKER_FLAGS="--coverage" ' +
                       '..'
                }
            }
        }
        stage ('Build') { 
            steps {
                dir ('build') {
                    sh "make -j${NR_JOBS}"
                    sh "make -j${NR_JOBS} runtests"
                }
            }
        }
        stage ('Build debug') { 
            steps {
                dir ('build-debug') {
                    sh "make -j${NR_JOBS}"
                    sh "make -j${NR_JOBS} runtests"
                }
            }
        }
        stage ('Test release') {
            steps {
                dir ('build') {

                    sh "make -j${NR_JOBS} check"

                    junit 'cqlite_testresults.xml' 
                }
            }
        }
        stage ('Test Coverage') {
            steps {
                dir ('build-debug') {
                    dir ('doc/coverage') {
                        echo 'Created the directory for the coverage report.'
                    }

                    sh "lcov --capture --initial --no-external --directory ${WORKSPACE} " +
                       "--output-file coverage_base.info"

                    sh "make -j${NR_JOBS} check"

                    sh """
                    lcov --capture --no-external --directory ${WORKSPACE} --output-file coverage.info
                    lcov --add-tracefile coverage_base.info --add-tracefile coverage.info --output-file coverage.info
                    lcov --remove coverage.info '*/tests/*' --output-file coverage.info
                    lcov --remove coverage.info '*/CMakeFiles/*' --output-file coverage.info

                    genhtml coverage.info -o doc/coverage
                    """

                    publishHTML (target: [
                        allowMissing: false,
                        alwaysLinkToLastBuild: true,
                        keepAll: true,
                        reportDir: 'doc/coverage',
                        reportFiles: 'index.html',
                        reportName: 'Coverage report'])
                }
            }
        }
        stage ('Documentation') {
            steps {
                dir ('build') {
                    sh 'make doc'

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

