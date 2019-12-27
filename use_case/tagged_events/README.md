# Use case: Tagged events

## Motivation
Sometimes we have event producer but at some point we would like
to reuse event producer for different configuration. In some cases we
could just add some "tag" to event to resolve this issue.

I would like to resolve this in other way as maybe data producer can't
be aware of its "tag". Other issue is spreading "tag" requirement over
whole project and maybe we don't want to modify old code.

It would be nice to just add some abstraction layer which will resolve
this issue.

