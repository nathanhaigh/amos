##-- CUSTOM AMOS RULES --##

##-- install-exec-hook
##   execute this custom rule after the global install-exec rule
##
install-exec-hook: install-bin-scripts-hook


##-- uninstall-hook
##   execute this custom rule after the global uninstall rule
##
uninstall-hook: uninstall-bin-scripts-hook



##-- install-bin-scripts-hook
##   takes list of scripts from $(dist_bin_SCRIPTS) $(bin_SCRIPTS) and:
##   1) locates the installed script
##   2) builds them with the below sed commands to assure the shebang and
##   include directives point to the correct locations, and that variable
##   substitution is performed as expected
##   3) strips their suffix (e.g. ".pl")
##
install-bin-scripts-hook: $(dist_bin_SCRIPTS) $(bin_SCRIPTS)
	@$(NORMAL_INSTALL)
	@list='$(dist_bin_SCRIPTS) $(bin_SCRIPTS)'; for p in $$list; do \
          f=`echo "$$p" | sed 's|^.*/||;$(transform)'`; \
          b=`echo "$$p" | sed 's|^.*/||;s|.[^.]*$$||;$(transform)'`; \
          if test -f "$(DESTDIR)$(bindir)/$$f"  &&  test "$$f" != "$$b"; then \
            $(INSTALL_SCRIPT) "$(DESTDIR)$(bindir)/$$f" "$(DESTDIR)$(bindir)/$$b" \
            || exit 1; \
            case "$$p" in \
              *.pl) \
                echo " configuring perl '$$b'"; \
                echo '#!$(PERL)' > "$(DESTDIR)$(bindir)/$$b"; \
                echo 'use lib "$(libdir)";' >> "$(DESTDIR)$(bindir)/$$b"; \
                sed -e '1 {s|^#!.*$$||;}' \
                    "$(DESTDIR)$(bindir)/$$f" >> "$(DESTDIR)$(bindir)/$$b" \
                || exit 1; \
                ;; \
              *.py) \
                echo " configuring python '$$b'"; \
                echo '#!$(PYTHON)' > "$(DESTDIR)$(bindir)/$$b"; \
                sed -e '1 {s|^#!.*$$||;}' \
                    -e 's|^AMOSDIR[     ]*=.*$$|AMOSDIR=\"$(bindir)/\"|' \
                    "$(DESTDIR)$(bindir)/$$f" >> "$(DESTDIR)$(bindir)/$$b" \
                || exit 1; \
                ;; \
              *.acf) \
                echo " configuring acf '$$b'"; \
                echo '#!$(bindir)/runAmos -C' > "$(DESTDIR)$(bindir)/$$b"; \
                sed -e 's|^BINDIR[ 	]*=.*$$|BINDIR=$(bindir)|' \
                    -e 's|^NUCMER[ 	]*=.*$$|NUCMER=$(NUCMER)|' \
                    -e 's|^DELTAFILTER[        ]*=.*$$|DELTAFILTER=$(DELTAFILTER)|' \
                    "$(DESTDIR)$(bindir)/$$f" >> "$(DESTDIR)$(bindir)/$$b" \
                || exit 1; \
                ;; \
              *) \
                echo " configuring '$$b'"; \
                cp "$(DESTDIR)$(bindir)/$$f" "$(DESTDIR)$(bindir)/$$b" \
                ;; \
            esac; \
            rm -f "$(DESTDIR)$(bindir)/$$f"; \
          else :; fi; \
        done


##-- uninstall-bin-scripts-hook
##   takes list of scripts from $(dist_bin_SCRIPTS) $(bin_SCRIPTS) and removes
##   their suffix-stripped versions.
##
uninstall-bin-scripts-hook:
	@$(NORMAL_UNINSTALL)
	@list='$(dist_bin_SCRIPTS) $(bin_SCRIPTS)'; for p in $$list; do \
          b=`echo "$$p" | sed 's|^.*/||;s|.[^.]*$$||;$(transform)'`; \
          echo " rm -f '$(DESTDIR)$(bindir)/$$b'"; \
          rm -f "$(DESTDIR)$(bindir)/$$b"; \
        done


##-- END OF MAKEFILE --##
