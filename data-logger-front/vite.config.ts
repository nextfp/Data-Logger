import path from "path";
import { defineConfig } from "vite";
import { VitePWA } from "vite-plugin-pwa";
import react from "@vitejs/plugin-react-swc";

const root = path.resolve(__dirname, "./src");
const outDir = path.resolve(__dirname, "dist");

// https://vitejs.dev/config/
export default defineConfig({
  root,
  plugins: [
    react(),
    VitePWA({
      registerType: "autoUpdate",
      includeAssets: ["favicon.ico", "apple-touch-icon.png"],
      injectRegister: "auto", //ここの記述
      manifest: {
        theme_color: "#f69435",
        background_color: "#f69435",
        display: "fullscreen",
        scope: "/indicator/",
        start_url: "/indicator/index.html",
        name: "indicator",
        short_name: "indicator",
        description: "next fp indicator",
        icons: [
          {
            src: "/vite.svg",
            sizes: "512x512",
            type: "image/svg+xml",
          },
        ],
      },
    }),
  ],
  resolve: {
    alias: {
      "@": path.resolve(__dirname, "./src"),
    },
  },
  build: {
    outDir,
    rollupOptions: {
      input: {
        main: path.resolve(root, "index.html"),
        indicator: path.resolve(root, "indicator/index.html"),
        any: path.resolve(root, "angle/index.html"),
        log: path.resolve(root, "log/index.html"),
      },
    },
  },
});
