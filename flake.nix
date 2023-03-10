{
  description = "tenchu-decomp";
  nixConfig.bash-prompt = "[nix-develop]$ ";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
  };

  outputs =
    inputs@{ self
    , nixpkgs
    , flake-utils
    , flake-compat

    }:
    flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [

        ];
      };
      mipselPkgs = pkgs.pkgsCross.mipsel-linux-gnu;

      rabbitizer = pkgs.python3Packages.buildPythonApplication rec {
        pname = "rabbitizer";
        # version = inputs.spimdisasm.rev;
        version = "1.5.10";
        # src = inputs.spimdisasm;
        src = pkgs.python3Packages.fetchPypi {
          inherit pname;
          inherit version;
          sha256 = "sha256-c0dtRm0/YtSNoGQQGpdUi/r7fgKOKGLGgje2XkP3tmE=";
        };


        propagatedBuildInputs = [
          # pkgs.python3Packages.rabbitizer
        ];
      };


      spimdisasm = pkgs.python3Packages.buildPythonApplication rec {
        pname = "spimdisasm";
        format = "pyproject";

        version = "1.11.5";

        src = pkgs.python3Packages.fetchPypi {
          inherit pname;
          inherit version;
          sha256 = "sha256-ACoNbRfW/vkCYc8JVhSKSZ7xDjODgO5yxzRqusSUTvY=";
        };


        propagatedBuildInputs = [
          rabbitizer
        ];
        nativeBuildInputs = [
          pkgs.python3Packages.setuptools
        ];
      };
      m2c = pkgs.python3Packages.buildPythonApplication rec {
        pname = "m2c";
        format = "pyproject";

        version = "0.4.7";

        src = pkgs.python3Packages.fetchPypi {
          inherit pname;
          inherit version;
          sha256 = "sha256-JEgs+vZcmNUaqIbECxRXqzssqTLrLda1iq4OX6GO05U=";
        };


        propagatedBuildInputs = [
          pkgs.python3Packages.pycparser
          pkgs.python3Packages.watchdog
          pkgs.python3Packages.psutil
        ];
        nativeBuildInputs = [
          pkgs.python3Packages.setuptools
        ];
      };
    in
    {
      legacyPackages = pkgs;
      packages = flake-utils.lib.flattenTree
        { };

      apps = { };

      devShell = mipselPkgs.mkShell {
        nativeBuildInputs = [
          pkgs.busybox
          spimdisasm
          m2c
        ];
        buildInputs = [

        ];
        LANG = "C.UTF-8";
      };
    });
}
