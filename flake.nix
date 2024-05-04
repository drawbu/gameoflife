{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = {self, ...} @ inputs:
    inputs.utils.lib.eachDefaultSystem (system: let
      pkgs = inputs.nixpkgs.legacyPackages.${system};
      cc = pkgs.gcc12;
    in rec {
      formatter = pkgs.alejandra;

      devShells.default = pkgs.mkShell {
        CC = "${cc}/bin/gcc";

        inputsFrom = [packages.default];
        packages = with pkgs; [valgrind gdb];
      };

      packages = {
        default = packages.server;
        server = pkgs.stdenv.mkDerivation {
          name = "gameoflife";
          src = ./.;

          CC = "${cc}/bin/gcc";
          buildInputs = with pkgs; [glibc gnumake];
          nativeBuildInputs = with pkgs; [raylib wayland];

          installPhase = ''
            mkdir -p $out/bin
            install -D $name $out/bin
          '';
        };
      };
    });
}
