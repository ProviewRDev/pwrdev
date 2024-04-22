#  dir_symbols.mk  -  get directory symbols

# This makefile sets some variables derived from the folder path structure:
# comp_name = Component name
# type_name = Type name
# root_name = root name

src_name	:= $(notdir $(PWD))
PARENT_DIR := $(patsubst %/,%,$(dir $(PWD))) 			# Fetch the parent directory of the current working directory
comp_name := $(notdir $(PARENT_DIR))					# Extract the component name
PARENT_DIR := $(patsubst %/,%,$(dir $(PARENT_DIR)))		# Fetch the parent directory once again from where PARENT_DIR currently is
type_name := $(notdir $(PARENT_DIR))					# Extract the type name
PARENT_DIR := $(patsubst %/,%,$(dir $(PARENT_DIR)))		# Fetch the parent directory once again from where PARENT_DIR currently is
root_name := $(notdir $(PARENT_DIR))					# Extract the root name

# Clean the variables from any whitespaces, tabs, newlines or carrige return
src_name := $(shell echo "$(src_name)" | tr -d ' \t\r\n')
comp_name := $(shell echo "$(comp_name)" | tr -d ' \t\r\n')
type_name := $(shell echo "$(type_name)" | tr -d ' \t\r\n')
root_name := $(shell echo "$(root_name)" | tr -d ' \t\r\n')
