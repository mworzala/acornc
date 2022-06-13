# Fuzzing


## GH Actions notes
Plan is to put this in a github action and run it for some amount of time every night on master.

If ./fuzzer.sh fails with exit code 77 then bad things happened. `artifacts` directory needs to be uploaded as a build artifact somewhere.

If possible it would be nice to run the minimizer on the fuzzed input before uploading as an artifact.