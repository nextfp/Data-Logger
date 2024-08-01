import path from "path";
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react-swc";

const root = path.resolve(__dirname, "./src");

// https://vitejs.dev/config/
export default defineConfig({
  root,
  plugins: [react()],
  resolve: {
    alias: {
      "@": path.resolve(__dirname, "./src"),
    },
  },
});
