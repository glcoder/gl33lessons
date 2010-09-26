LESSONS := lesson01

all: force_look
	@for LESSON in $(LESSONS); do echo "Building $$LESSON..."; \
		(cd $$LESSON; make all); done

clean: force_look
	@for LESSON in $(LESSONS); do echo "Cleanup $$LESSON..."; \
		(cd $$LESSON; make clean); done

force_look:
	@true
