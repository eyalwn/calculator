#	Filename	:	makefile
#	Developer	:	Eyal Weizman
#	Last Update	:	2020-02-25
#	Description	:	makefile for the calculator directory

################# vairables #######################
# compiler flags
flags = -pedantic-errors -Wall -Wextra -g -Og
end_flags = -lm

# files
app_src = calc_app.c
test_src = calc_test.c
sources = calc.c stack/stack.c
headers = calc.h stack/stack.h

# out files
test_out = test.out
app_out = calc.out


################ main commands ####################
.PHONY : app test clean

app : $(app_out) 

test : $(test_out) 

clean:
	rm -f *.o *.out


################ secondary rules ####################
$(test_out) : $(test_src) $(sources) $(headers)
	cc $(flags) $< $(sources) -o $@ $(end_flags)

$(app_out) : $(app_src) $(sources) $(headers)
	cc $(flags) $< $(sources) -o $@ $(end_flags)
