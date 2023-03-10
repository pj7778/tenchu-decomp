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
    in
    {
      legacyPackages = pkgs;
      packages = flake-utils.lib.flattenTree
        { };

      apps = { };

      devShell = mipselPkgs.mkShell {
        nativeBuildInputs = [

        ];
        buildInputs = [

        ];


        LANG = "C.UTF-8";
      };
    });
}
