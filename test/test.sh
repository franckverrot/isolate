#!/usr/bin/env bash

before_no_acl() {
  rm -rf tmp
  mkdir tmp
  touch tmp/test
}

before_with_acl() {
  before_no_acl
  echo "tmp/test" > tmp/acl-ruby
}


test_acl() {
  category=$1
  test_case=$2
  result=$3
  expected=$4

  if [[ "$result" == $expected ]]; then
    status="SUCCESS"
  else
    status="FAIL"
    debug="(result: $result)"
  fi

  printf "%-8s -- %-55s -- %-8s $debug\e[0m\n" "$category" "$test_case" "$status"

}

###### Without ACL ##################
before_no_acl
test_case="non interactive mode doesn't allow for file reading"
result=$(./isolate -f tmp/acl-ruby "cat tmp/test 2>/dev/null"; echo $?)
test_acl "no acl" "$test_case" $result 1

before_no_acl
test_case="non interactive mode doesn't allow for file writing"
result=$(./isolate -f tmp/acl-ruby "sed -i "s/STRING_TO_REPLACE/STRING_TO_REPLACE_IT/g" tmp/test 2>/dev/null"; echo $?)
test_acl "no acl" "$test_case" $result 1

before_no_acl
test_case="non interactive mode doesn't allow for file deletion"
result=$(./isolate -f tmp/acl-ruby "ruby -e \"File.unlink('tmp/test')\" 2>/dev/null"; echo $?)
test_acl "no acl" "$test_case" $result 1
###### Without ACL ##################


###### With ACL ##################
before_with_acl
test_case="non interactive mode doesn't allow for file reading"
result=$(./isolate -f tmp/acl-ruby "cat tmp/test 2>/dev/null"; echo $?)
test_acl "with acl" "$test_case" $result 0

before_with_acl
test_case="non interactive mode doesn't allow for file writing"
result=$(./isolate -f tmp/acl-ruby "touch tmp/test 2>/dev/null"; echo $?)
test_acl "with acl" "$test_case" $result 0

before_with_acl
test_case="non interactive mode doesn't allow for file deletion"
result=$(./isolate -f tmp/acl-ruby "rm tmp/test"; echo $?)
test_acl "with acl" "$test_case" $result 0
###### With ACL ##################
