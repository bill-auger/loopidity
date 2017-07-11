
# NOTE: packaging is strongly coupled to the github tarball URL and naming scheme
GITHUB_REPO_NAME='loopidity'
PACKAGING_BRANCH='obs'
KEY_MD5SUM='b279da23d11f172c25722a681fdac250'
FAUXSUM='0123456789abcdef0123456789abcdef                                '
FAUXSIZE='1234567'

MINOR_VERSION_REGEX='^v[0-9]*\.[0-9]*$'
REVISION_REGEX='^v[0-9]*\.[0-9]*\.[0-9].*$'
VERSION_TAG_ERROR_MSG="could not locate a minor version tag of the form: vMAJOR.MINOR"
REV_TAG_ERROR_MSG="could not locate a revision tag of the form: vMAJOR.MINOR.REV"


# file locations
MAKE_FILE=src/Makefile
SERVICE_FILE=obs/_service
PKGBUILD_FILE=obs/PKGBUILD
SPEC_FILE=obs/loopidity.spec
DSC_FILE=obs/loopidity.dsc

# sanity checks
GIT_USER=$(git config user.name)
GPG_KEY=$(git config user.signingkey)
[ "$GIT_USER" ] || (echo "git config user.name - not set"       ; exit 1)
[ "$GPG_KEY"  ] || (echo "git config user.signingkey - not set" ; exit 1)

# get existing revision tags
MINOR_VERSION_TAG=$(git tag --list --merged    HEAD   | grep -G $MINOR_VERSION_REGEX | tail -n1)
LAST_REVISION_TAG=$(git tag --list --merged    HEAD   | grep -G $REVISION_REGEX      | tail -n1)
REVISION_TAGS=$(    git tag --list --no-merged master | grep -G $REVISION_REGEX                )

# detect commit to packaging branch and validate version tags
if [ "$(git rev-parse --abbrev-ref HEAD)" != "$PACKAGING_BRANCH" ]
then IS_PACKAGING_BRANCH=0 ; [ -z "$MINOR_VERSION_TAG" ] && echo $VERSION_TAG_ERROR_MSG && exit 1 ;
else IS_PACKAGING_BRANCH=1 ; [ -z "$LAST_REVISION_TAG" ] && echo $REV_TAG_ERROR_MSG     && exit 1 ;
fi


# comupte next revision string
if ! (($IS_PACKAGING_BRANCH))
then [ "$(ps -p $PPID -o command= | grep '\-\-amend')" ] && INCREMENT_REVISION=0 || INCREMENT_REVISION=1
     N_REVISIONS_AHEAD=$(git rev-list $MINOR_VERSION_TAG..HEAD --count)
     VERSION_STRING="$MINOR_VERSION_TAG.$(printf '%03d\n' $(($N_REVISIONS_AHEAD + INCREMENT_REVISION)))"
else VERSION_STRING=$LAST_REVISION_TAG
fi
VERSION=${VERSION_STRING//v}

# echo "MINOR_VERSION_TAG=$MINOR_VERSION_TAG"
# echo "N_REVISIONS_AHEAD=$N_REVISIONS_AHEAD"
