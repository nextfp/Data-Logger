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
        start_url: "/indicator/",
        name: "indicator",
        short_name: "indicator",
        description: "next fp indicator",
        icons: [
          {
            src: "/icon-192x192.png",
            sizes: "192x192",
            type: "image/png",
          },
          {
            src: "/icon-256x256.png",
            sizes: "256x256",
            type: "image/png",
          },
          {
            src: "/icon-384x384.png",
            sizes: "384x384",
            type: "image/png",
          },
          {
            src: "/icon-512x512.png",
            sizes: "512x512",
            type: "image/png",
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
      },
    },
  },
});
