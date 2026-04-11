
// Firebase Configuration
const firebaseConfig = {
  apiKey: "AIzaSyDyIr2fg2uUtJPeHmvTJhePkt6DWti12Vw",
  authDomain: "page-not-found-a7dcb.firebaseapp.com",
  projectId: "page-not-found-a7dcb",
  storageBucket: "page-not-found-a7dcb.firebasestorage.app",
  messagingSenderId: "59141562173",
  appId: "1:59141562173:web:a724b5c22145e2b974ef54"
};

// Initialize Firebase (Compat version)
firebase.initializeApp(firebaseConfig);
const auth = firebase.auth();
const googleProvider = new firebase.auth.GoogleAuthProvider();

// GOOGLE LOGIN FUNCTION
function googleLogin() {
    auth.signInWithPopup(googleProvider)
    .then((result) => {
        console.log("Google User:", result.user);
        window.location.href = "index.html"; // Redirect to your homepage
    }).catch((error) => {
        alert("Google Error: " + error.message);
    });
}

// EMAIL SIGN UP FUNCTION
function signUpUser(email, password) {
    auth.createUserWithEmailAndPassword(email, password)
    .then((userCredential) => {
        alert("Account Created!");
        window.location.href = "logIn.html";
    }).catch((error) => {
        alert(error.message);
    });
}

// EMAIL LOG IN FUNCTION
function logInUser(email, password) {
    auth.signInWithEmailAndPassword(email, password)
    .then((userCredential) => {
        window.location.href = "index.html"; 
    }).catch((error) => {
        alert(error.message);
    });
}

// Attach Event Listeners (Once the DOM is ready)
document.addEventListener("DOMContentLoaded", () => {
    // Handle Sign Up Form
    const signupForm = document.getElementById("signupForm");
    if (signupForm) {
        signupForm.addEventListener("submit", (e) => {
            e.preventDefault();
            const email = signupForm.querySelector('input[type="email"]').value;
            const password = document.getElementById("password").value;
            signUpUser(email, password);
        });
    }

    // Handle Login Form
    const loginForm = document.querySelector(".login_form form");
    if (loginForm) {
        loginForm.addEventListener("submit", (e) => {
            e.preventDefault();
            const email = document.getElementById("email").value;
            const password = document.getElementById("password").value;
            logInUser(email, password);
        });
    }

    // Handle Google Buttons (Both pages)
    const googleBtns = document.querySelectorAll(".google-btn, .option a");
    googleBtns.forEach(btn => {
        btn.onclick = (e) => {
            e.preventDefault();
            googleLogin();
        };
    });
});