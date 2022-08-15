# DOCKER how-to


1. Run the Docker build command:

	```
	docker build -f Dockerfile . -t ntd-model-lf
	```

1. Now you can run the Docker container `ntd-model-lf` where required. The container expects the repo code to be present under `/ntd/model`, so to run it locally and mount the (current) repo directory as `/ntd/model`, use:

    ```
    docker run -v ${PWD}:/ntd/model -ti ntd-model-lf
    ```    
    
1. This gives you a `bash` shell inside the container where you can run `transfil_N` from any directory. You can also run the model from the host using `transfil_N <...args...>`. To run it from the host, use:
	```
	docker run -v ${PWD}:/ntd/model -ti ntd-model-lf transfil_N <...args...>
	```
	
1. When running from the host, remember to prefix any arguments (e.g. `-s <scenarios_file>`) with the location of the folder relative to the mounted directory (e.g. `-s 'C++ code'/testscenario.xml` if you mount the repo folder as above). You can mount any local folder you like as `/ntd/model` (using e.g. `-v ${PWD}:/ntd/model` in the `docker run` command) and the container will treat that as the current working directory (i.e. `.`).