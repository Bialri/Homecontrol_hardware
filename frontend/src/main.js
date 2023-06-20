import { createApp } from 'vue'
import App from './App.vue'
import router from "@/router";
import './interceptors/axios'

import './assets/bootstrap/css/bootstrap.min.css';
import './assets/bootstrap/js/bootstrap.min'
import './assets/css/styles.min.css'
import './assets/js/script.min'

createApp(App).use(router).mount('#app')
