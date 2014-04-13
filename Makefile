all clean:
	@for dir in $$(ls); do \
		if [ -d $$dir ]; then \
			$(MAKE) -C $$dir $@;\
		fi; \
	done;
