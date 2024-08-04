import path from "path";
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react-swc";

const root = path.resolve(__dirname, "./src");
const outDir = path.resolve(__dirname, "dist");

// https://vitejs.dev/config/
export default defineConfig({
  root,
  plugins: [react()],
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
