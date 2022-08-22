# DOCKER how-to


1. Run the Docker build command:

	```
	docker build -f Dockerfile . -t ntd-model-lf
	```

1. Now you can run the Docker container `ntd-model-lf` where required. The container has a working directory of `/ntd/run` which is basically the contents of `./run`, and will write its output files into `/ntd/run/combined_output`, so you should create a local folder somewhere and mount it there on the container:

    ```
    mkdir /tmp/combined_output
    docker run -v /tmp/combined_output:/ntd/run/combined_output -ti ntd-model-lf
    ```    
    
1. This gives you a `bash` shell inside the container where you can run `transfil_N` from any directory. You can also run the model from the host using `transfil_N <...args...>`. To run it from the host, use:
2. 
	```
	docker run -v ${PWD}:/ntd/model -ti ntd-model-lf transfil_N <...args...>
	```
	
1. When running the container's `transfil_N` from the host using `docker run`, remember to prefix any arguments (e.g. `-s <scenarios_file>`) with the location of the folder relative to the mounted directory (e.g. `-s 'C++ code'/testscenario.xml` if you mount the repo folder as above).

1. You can also use the included `run` scripts, which work with the packaged `parameters` and `scenarios` folders inside `./run` and copied into the container under `/ntd/run`. This will run the model on an individual ID:

	```
	bash run-lf-model.bash <output-folder-name> <running-ID>
	```
	Whereas this will run the model on all the IDs in the `running-id-list.txt` file in parallel across as many cores as the host computer has available:
	
	```
	bash run-in-parallel.bash <output-folder-name>
	```