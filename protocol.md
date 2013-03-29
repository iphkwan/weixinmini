# command

`[(A) | (B)]`: A or B
`(A)*`: zero or more A
`[gid]`: zero or one gid

## Register
C: register nickname passwd
S: [(ok uid) | (error reason)]

## Login
C: login uid passwd
S: [(ok) | (error reason)]

## Friend
C: fadd fid
C: fdel fid
S: [(ok) / (error reason)]

C: fsearch nickname
S: (fid nickname)*

C: fsend fid message
S: [(ok) | (error reason)]

## Group
C: gadd nickname
S: [(ok gid) | (error reason)]

C: gdel gid
S: [(ok) | (error reason)]

C: ginv gid (uid)*
C: gsend gid message
S: [(ok) | (error reason)]

# Protocol

## request = uid command
e.g.
C: uid fadd fid

## response = response command
e.g.
S: response ok

## push = message content uid [gid]
e.g.
S: message hello uid
S: message hello uid gid
