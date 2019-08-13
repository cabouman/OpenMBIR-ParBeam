if which brew > /dev/null; then
	# The package is installed
	echo Brew Installed
else
	# Install brew
	echo Installing Brew
	/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
fi

if command -v python3 > /dev/null 2>&1; then
    # Python 3 installed
    echo Python 3 Installed
else
    # Install python3
    echo Installing Python 3
    brew install python3
fi

# Install pyqt5
if brew list pyqt5 > /dev/null; then
	# The package is installed
	echo PyQT 5 Installed
else
	# Install pyqt5
	echo Installing PyQT 5
	brew install pyqt5
fi

# Install numpy
if brew list numpy > /dev/null; then
	# The package is installed
	echo Numpy Installed
else
	# Install numpy
	echo Installing Numpy
	brew install numpy
fi

# Install scipy
if brew list scipy > /dev/null; then
	# The package is installed
	echo SciPy Installed
else
	# Install scipy
	echo Installing SciPy
	brew install scipy
fi

