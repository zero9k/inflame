![CI on ubuntu](https://github.com/zero9k/inflame/workflows/CI%20on%20ubuntu/badge.svg)
![CI on OSX](https://github.com/zero9k/inflame/workflows/CI%20on%20OSX/badge.svg)
![CI on Windows](https://github.com/zero9k/inflame/workflows/CI%20on%20Windows/badge.svg)
![CodeQL](https://github.com/zero9k/inflame/workflows/CodeQL/badge.svg)
![CI with Sanitizer](https://github.com/zero9k/inflame/workflows/CI%20with%20Sanitizer/badge.svg)

# InFlame
Driver for Im-Mortal, Based on [FluffOS](https://github.com/fluffos/fluffos).

## Build
```shell
# Install all libs
$ sudo apt update
$ sudo apt install build-essential bison libevent-dev libmysqlclient-dev libpcre3-dev libpq-dev libsqlite3-dev libssl-dev libz-dev libjemalloc-dev libicu-dev

# Clone Git Repo
$ git clone https://github.com/zero9k/inflame.git
$ cd inflame

# Upgrade cmake
$ sudo pip install --upgrade cmake

# Modify src/local_options, pay attention to the following: 
# 1. NO_ADD_ACTION
# 2. NO_LIGHT
# 3. SENSIBLE_MODIFIERS
# 4. PRIVS
# 5. USE_ICONV

# Build
$ mkdir build
$ cd build
$ cmake .. -DPACKAGE_DWLIB=OFF
$ make -j4 install
```
