{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { ... }@inputs:
    inputs.utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import inputs.nixpkgs { inherit system; };
        inherit (pkgs) lib;
      in
      rec {
        formatter = pkgs.nixfmt-tree;

        devShells.default = pkgs.mkShell {
          inputsFrom = builtins.attrValues packages;
          packages = with pkgs; [ ];
        };

        packages = {
          default = packages.server;
          server = pkgs.gcc15Stdenv.mkDerivation {
            name = "gameoflife";
            src = lib.fileset.toSource {
              root = ./.;
              fileset = lib.fileset.unions [
                ./gameoflife.c
                ./Makefile
              ];
            };

            buildInputs = with pkgs; [ pkg-config ];
            nativeBuildInputs = with pkgs; [ raylib ];

            installPhase = ''
              mkdir -p $out/bin
              install -Dm755 $name $out/bin
            '';
          };
        };
      }
    );
}
