Src := $(wildcard Src/*.cpp) $(wildcard Src/*.h)
Doc := ($ wildcard Doc/*)
cpc := g++

all: Doc todo-cli
doc: README.md  $(Doc:*.md)
	@Scripts/parseAllMd.sh
todo-cli: $(Src)
	@$(cpc) $(Src) -Wall -lncurses -o todo-cli
install:
	@cp ./todo-cli /usr/bin
clean:
	@rm -rf todo-cli *.o *.html Doc/*.html
