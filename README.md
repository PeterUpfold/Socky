Socky
=====

Listen on TCP sockets to observe connection attempts to servers on the local network.

This is designed to provide some level of visibility into any activities on your system where local servers
running on Well-Known (and merely well-known, if you catch my drift) ports are being enumerated.

Inspired by the proof of concept of this demonstrated by https://wybiral.github.io/wtf/

**Note:** that this is not particularly serious or practical, but I wanted a project that let me work directly
with the Core Foundation APIs in C, and this seemed a good opportunity.

Installation
============

 * Build and copy `Socky` to `/usr/local/bin/Socky` (or another appropriate path).
 * Review `uk.org.upfold.Socky.plist`. In `ProgramArguments`, supply as many ports on which you wish Socky to bind and listen for incoming connections. Note that these ports must not be occupied (such as the `27060` port which appears to be used by Steam).
 * Copy `uk.org.upfold.Socky.plist` to `~/Library/LaunchAgents` to have Socky start on login. Remember that it will bind to the ports specified, so will stop any other application from binding to those ports!
