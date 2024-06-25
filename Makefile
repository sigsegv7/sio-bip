CFILES = $(shell find . -name "*.c")

bin/bip-test: $(CFILES)
	mkdir -p $(@D)
	gcc $^ -o $@
