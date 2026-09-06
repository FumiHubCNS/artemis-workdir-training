{
  description = "artemis execution environment";

  inputs = {
    nixpkgs.url = "nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    artemis-flake = {
      url = "github:FumiHubCNS/artemis-flake";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, artemis-flake }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          name = "my-artemis-project";
          packages = [
            artemis-flake.packages.${system}.artemis
          ];
        };
      }
    );
}
