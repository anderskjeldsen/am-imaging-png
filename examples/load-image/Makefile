# place amlc.jar in this folder or change value.
AMLC:=amlc.jar
CMD=java -jar $(AMLC)

build-linux-x64:
	$(CMD) build . -bt linux-x64 -ll5 -maxOneError

run-linux-x64: copy-assets
	$(CMD) run . -bt linux-x64 -ll5 -maxOneError

build-amigaos:
	$(CMD) build . -bt amigaos_docker -ll5 -maxOneError

deps: # force load dependencies
	$(CMD) deps . -fld

builds/bin/linux-x64/cookie_8bit.png: assets/cookie_8bit.png
	mkdir -p builds/bin/linux-x64
	cp assets/cookie_8bit.png builds/bin/linux-x64/cookie_8bit.png

copy-assets: builds/bin/linux-x64/cookie_8bit.png

clean:
	$(CMD) clean . -bt amigaos_docker
	$(CMD) clean . -bt linux-x64