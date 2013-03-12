all:
	@cd ./rain-src/ && make all
	@cd ./routine-src/ && make all
clean:
	@cd ./rain-src/ && make clean
		@cd ./routine-src/ && make clean