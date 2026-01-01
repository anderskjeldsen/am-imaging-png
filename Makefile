# place amlc.jar in this folder or change value.
AMLC:=amlc.jar
CMD=java -jar $(AMLC)

build-linux-x64:
	$(CMD) build . -bt linux-x64 -ll 5 -maxOneError

run-linux-x64:
	$(CMD) run . -bt linux-x64 -ll5 -maxOneError

build-macos-arm:
	$(CMD) build . -bt macos-arm -ll5 -maxOneError

build-amigaos:
	$(CMD) build . -bt amigaos_docker -ll5 -maxOneError

deps: # force load dependencies
	$(CMD) deps . -fld

clean:
	$(CMD) clean . -bt amigaos_docker
	$(CMD) clean . -bt linux-x64