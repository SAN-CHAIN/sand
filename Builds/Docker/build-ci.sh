set -e

if [ -z "$GIT_SHA1" ]; then
  GIT_SHA1=`git rev-parse --short HEAD`
fi
if [ -z "$PROJECT" ]; then
  PROJECT=tfcc/tfccd
fi

mkdir -p build/docker/

cp build/rippled build/docker/
cp build/rippled Builds/Docker/*.cfg build/docker/
cp Builds/Docker/entry.sh build/docker/
cp doc/rippled.logrotate build/docker/rippled.logrotate
chmod 644 build/docker/rippled.logrotate
cp Builds/Docker/Dockerfile-catalog build/docker/Dockerfile

strip build/docker/rippled
docker build -t $PROJECT:$GIT_SHA1 build/docker/
docker tag $PROJECT:$GIT_SHA1 $PROJECT:latest

if [ -n "$BRANCH" ]; then
  docker tag $PROJECT:$GIT_SHA1 $PROJECT:$BRANCH
fi
