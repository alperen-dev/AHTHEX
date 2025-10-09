# Batch file name
BATCH = WINBUILD.BAT

# Default target
all: run

# Run the batch file
run:
	$(BATCH)

# Clean rule (if you want to remove generated files from the .bat)
clean:
	echo Cleaning... (nothing to delete here)
