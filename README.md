# isolate

`isolate` is a command-line tool that prevents the commands it's wrapping from
accessing unauthorized files.

It is currently whitelist-oriented, the list of files a process will be allowed
to access is located in the current directory: `$(pwd)/.access-control`

`isolate` has been tested on Ubuntu 14.04 and OSX (Mavericks and Yosemite).

An interactive mode is also available that allows users to interact *at runtime*
with the files accessed by their processes.

## Installation

1. Clone the official repo: `git clone https://github.com/franckverrot/isolate.git`
2. `make` will both compile and run the test suite

## Usage

### ACL file

By default, a `.access-control` ACL file in the current working directory will be
used when running isolate. It can be overriden by using the `-f` flag to
specify another ACL file.

### Isolating processes
    isolate "your command and options"
    isolate -i -- "your command and options"
    isolate -i -f your-acl-file -- "your command and options"

### CLI options

* `-V` : verbose mode, will display extra info
* `-f <acl file>` : specify another ACL file
* `-i` : run in interactive mode

## TODO

1. [ ] Make a cross-platform installation package (w/ `aclocal`/`autoconf`/`autoheader`/`automake`)
2. [ ] Submit to `homebrew` for OSX
3. [ ] Create a Debian package
4. [ ] Rework the interactive mode

## Contributing

1. Fork it ( https://github.com/franckverrot/isolate/fork )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
