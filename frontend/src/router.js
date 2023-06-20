import {createRouter,createWebHashHistory} from "vue-router";
import IndexPage from "@/components/IndexPage.vue";
import RegPage from "@/components/RegPage.vue";

const routes = [
    { path: '/', component: IndexPage },
    { path: '/reg', component: RegPage },
]

export default createRouter({
    history: createWebHashHistory(),
    routes: routes
})