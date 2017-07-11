
the files contained in this directory are used by the OpenSUSE Build Service
to build/package binary releases and are otherwise not interesting


initial local configuration:
* ensure that the custom git hooks are installed locally  
  => $ git config --local core.hooksPath hooks

for each release version:
* ensure that the appropriate tag 'vMAJOR.MINOR' exists on the development branch  
  or add a new tag 'vMAJOR.MINOR' if major or minor version should change
* the git hooks should have put the tag 'vMAJOR.MINOR.REV' on the HEAD  
  where REV is n_commits after MINOR
* checkout the 'debian' branch
* rebase onto the development branch that is the release candidate
* update and commit the changelog at the very least before continuing this procedure
* checkout the 'obs' branch to enable the packaging-specific git hooks
* rebase onto the development branch that is the release candidate
* in obs/debian.changelog  
  => add new entry for this version with a list of significant commits
* trigger the git hooks with a new commit  
  => $ git commit --allow-empty -m 'update packaging files to vMAJOR.MINOR.REV'
* copy all files in the obs/ directory into the local osc directory

_NOTE: after each commit:_
* some files under the obs/ directory will necessarily be dirty - ignore these
* a tarball identical to the github "tag release" will be in the obs/ directory
* signatures for the tarball and PKGBUILD will be in the obs/ directory
* the PKGBUILD recipe will be coupled to this tarball, checksums, and signatures
* the .spec and .dsc recipes will be coupled to this tarball and checksums

if build or install steps have changed:
* in loopidity.spec  
  => update '%build' recipe, and/or '%post' install hooks
* in debian.rules  
  => update 'build-stamp:' and 'install:' recipes
* in PKGBUILD  
  => update 'build()' and 'package()' recipes  
  => $ gpg --detach-sign PKGBUILD

if output files have changed:
* in loopidity.spec  
  => update package '%files'

if dependencies have changed:
* in loopidity.spec  
  => update 'BuildRequires' and/or 'Requires'
* in loopidity.dsc  
  => update 'Build-Depends'
* in debian.control  
  => update 'Build-Depends' and/or 'Depends'
* in PKGBUILD  
  => update 'makedepends' and/or 'depends'  
  => $ gpg --detach-sign PKGBUILD

packaging:
* create remote tarball and explicit github "release"  
  => $ git push upstream --tags
* upload PKGBUILD.sig and TARBALL.sig to new github "release"
* delete PKGBUILD.sig, TARBALL.sig, and TARBALL
* osc local build - e.g.  
  => $ osc build openSUSE_Tumbleweed i586   ./loopidity.spec  
  => $ osc build Debian_8.0          x86_64 ./loopidity.dsc
* tweak any of the preceding steps as necessary until everything rocks sweetly
* verfy signature of downloaded github "release" tarball
* osc commit to the OBS build server for production build

commit packaging files to git:
* copy any modified files back into obs/ directory and amend commit if necessary
* revert the files carrying checksums that need not (and can not) be committed
  => $ git checkout HEAD obs/
* checkout the master branch and fast-forward to the development branch  
  => $ git checkout master && git merge development
