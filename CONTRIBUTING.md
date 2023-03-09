### Loopidity Contribution Guidelines

All files in the Loopidity code-base are distributed as [Free Software][free-sw] and [Free Culture][free-culture] under the terms of the [version 3 of the GNU General Public License][gpl] (GPLv3). To this end, it is necessary for all contributions to be offered under a GPL-compatible license. If no license is specified, the contribution will be considered to be under the same license as Loopidity.

In short, this means that as a contributor, you allow others to copy, distribute, and modify their copies or your work; provided that they extend this privilege to others, and credit you as the original author. This implies that all source files are made available in their respective preferred forms for modification. For source code, that is simply the source code itself. For multimedia and documentation, this is all individual source files and/or editor project files in addition to any "mixed-down" binary artifacts. Refer to the [multimedia README][data-readme] for details.


### Contribution Licensing Terms

As a contributor, you agree that all contributions to the source tree, wiki, and issue tracker that are unlicensed and not in the public domain will automatically fall under the same licence as Loopidity.

If you are not the sole author of your contribution (i.e. it is a combined or derivative work incorporating or based on someone else's work), then all source works must be freely distributable under the terms of some [GPL-compatible license][license-list] such as the Creative Commonns CC-BY or in the public domain; and you must clearly give attribution to the copyright holders with hyperlinks to the original sources and licenses.

To be clear, this is not at all a special case of this project. It is the commonly accepted "inbound<->outbound" tacet norm for free and open-source software projects; and is even made explicit by some public services such as [item 'D6' of the Github Terms of Service][github-tos].

For multimedia and other binaries, the GPL licensing terms are explicitly defined in the [multimedia README][data-readme]. Please read and understand this before submitting artwork and other prepared media. Contributions that do not meet the licensing criteria will not be accepted.


### Pull Requests

* Developers: please issue pull requests against the upstream 'development' branch.
* Designers: please issue pull requests against the upstream 'design' branch.

Note that branches other than 'master' tend to be rebased often; so you may need to force pull those. Please rebase all pull requests onto the latest 'development' or 'design' HEAD, and squash trivial commits; but try to retain commits representing significant or related changes (see example below). Ideally in this way, all upstream branches should be a fast-forward from 'master' and so re-sync should be simple.


### Repo Mirrors

Pull requests and issues are accepted at any of the following mirrors:

* [https://github.com/bill-auger/loopidity/][github]
* [https://notabug.org/bill-auger/loopidity/][notabug]
* [https://pagure.io/loopidity/][pagure]
* [https://sourceforge.net/projects/loopidity/][sourceforge]


### Commit Messages

Please add commit messages of the following form:
```
short general description of feature in declarative tense (optional issue #)
<EMPTY LINE>
  * specific notable change
  * specific notable change
  * specific notable change
  * specific notable change

e.g.

add bar powers to the mighty foo (issue #42)

  * added Bar class
  * added height, width instance vars to Bar class
  * added Bar instance var to Foo class
  * added setBar(), getBar() accessors to Foo class
```


### Issue Tracker

Please do not hesitate to use the issue tracker for bug reports, feature requests, constructive notes on existing issues, and significant progress updates that are not yet in a pull request; but use the [gitter chat][gitter] or other channels for lengthy discussions and other issues such as general help.


[free-sw]:       https://www.gnu.org/philosophy/free-sw.html
[free-culture]:  http://freedomdefined.org/Definition
[gpl]:           COPYING
[license-list]:  https://www.gnu.org/licenses/license-list.html#GPLCompatibleLicenses
[github-tos]:    https://web.archive.org/web/20170814143643/https://help.github.com/articles/github-terms-of-service/#6-contributions-under-repository-license
[data-readme]:   data/README.md
[github]:        https://github.com/bill-auger/loopidity/
[notabug]:       https://notabug.org/bill-auger/loopidity/
[pagure]:        https://pagure.io/loopidity/
[sourceforge]:   https://sourceforge.net/projects/loopidity/
[gitter]:        https://gitter.im/bill-auger/loopidity
