#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
using namespace std;
struct Date {
    int jour;
    int mois;
    int annee;
};
struct Escompte {
    string numero;
    string tireur;
    double montant;
    string dateEcheance;
    string lieuPaiement;
    string banqueDomiciliatrice;
    int joursEscompte;
    double escompte;
    double commission;
    double agios;
};
class BordereauEscompte {
private:
    vector<Escompte> escomptes;
    string dateRemise;
    string lieuBanque;
    string nomBanque;
    double tauxEscompte;
    double tauxTVA;
    double commPD, commPND, commDD, commDND;
    double commissionFixe;
    double commissionEndos;
    Date stringToDate(string dateStr) {
        Date d;
        sscanf(dateStr.c_str(), "%d/%d/%d", &d.jour, &d.mois, &d.annee);
        return d;
    }
    bool estBissextile(int annee) {
        return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
    }
    int joursParMois(int mois, int annee) {
        int jours[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (mois == 2 && estBissextile(annee))
            return 29;
        return jours[mois - 1];
    }
    int dateEnJours(Date d) {
        int totalJours = 0;
        for (int a = 1; a < d.annee; a++) {
            totalJours += estBissextile(a) ? 366 : 365;
        }
        for (int m = 1; m < d.mois; m++) {
            totalJours += joursParMois(m, d.annee);
        }
        totalJours += d.jour;
        return totalJours;
    }
    int calculerJours(string dateEch, string dateRem, bool surPlace) {
        Date echeance = stringToDate(dateEch);
        Date remise = stringToDate(dateRem);
        int joursEcheance = dateEnJours(echeance);
        int joursRemise = dateEnJours(remise);
        int joursCalendaires = joursEcheance - joursRemise;
        int joursBanque = surPlace ? 2 : 4;
        return joursCalendaires + joursBanque;
    }
    double calculerEscompte(double montant, int jours, double taux) {
        return (montant * taux * jours) / 36000;
    }
    double calculerCommission(bool surPlace, bool memeBanque) {
        if (surPlace && memeBanque)
            return commPD;
        else if (surPlace && !memeBanque)
            return commPND;
        else if (!surPlace && memeBanque)
            return commDD;
        else
            return commDND;
    }
    string toLower(string str) {
        for (char &c : str) {
            c = tolower(c);
        }
        return str;
    }
public:
    BordereauEscompte(string date, string lieu, string banque, double taux, double tva,
                      double cpd, double cpnd, double cdd, double cdnd,
                      double commFixe, double commEndos) 
        : dateRemise(date), lieuBanque(lieu), nomBanque(banque), 
          tauxEscompte(taux), tauxTVA(tva),
          commPD(cpd), commPND(cpnd), commDD(cdd), commDND(cdnd),
          commissionFixe(commFixe), commissionEndos(commEndos) {}
    void ajouterEscompte(string num, string tir, double mont, string dateEch, 
                         string lieu, string banque) {
        Escompte e;
        e.numero = num;
        e.tireur = tir;
        e.montant = mont;
        e.dateEcheance = dateEch;
        e.lieuPaiement = lieu;
        e.banqueDomiciliatrice = banque;
        bool surPlace = (toLower(lieu) == toLower(lieuBanque));
        bool memeBanque = (toLower(banque) == toLower(nomBanque));
        e.joursEscompte = calculerJours(dateEch, dateRemise, surPlace);
        e.escompte = calculerEscompte(mont, e.joursEscompte, tauxEscompte);
        e.commission = calculerCommission(surPlace, memeBanque) + commissionFixe + commissionEndos;
        e.agios = e.escompte + e.commission;
        escomptes.push_back(e);
    }
    void afficherBordereau() {
        cout << "\n";
        cout << "========================================================================\n";
        cout << "                    BORDEREAU D'ESCOMPTE BANCAIRE                       \n";
        cout << "========================================================================\n\n";
        cout << "Date de remise: " << dateRemise << "\n";
        cout << "Lieu: " << lieuBanque << "\n";
        cout << "Banque: " << nomBanque << "\n";
        cout << "Taux d'escompte: " << fixed << setprecision(2) << tauxEscompte << "%\n";
        cout << "Taux TVA: " << tauxTVA << "%\n";
        cout << "Commission fixe: " << commissionFixe << " DT\n";
        cout << "Commission d'endos: " << commissionEndos << " DT\n\n";
        cout << "+--------+------------+------------+------------+----------+--------+------------+------------+\n";
        cout << "| N effet| Valeur     | Lieu       | Echeance   | Banque   | Jours  | Escompte   | Commission |\n";
        cout << "+--------+------------+------------+------------+----------+--------+------------+------------+\n";
        double totalNominal = 0;
        double totalAgios = 0;
        for (const auto& e : escomptes) {
            totalNominal += e.montant;
            totalAgios += e.agios;
            cout << "| " << left 
                 << setw(7) << e.numero
                 << "| " << setw(11) << fixed << setprecision(3) << e.montant
                 << "| " << setw(11) << e.lieuPaiement
                 << "| " << setw(11) << e.dateEcheance
                 << "| " << setw(9) << e.banqueDomiciliatrice
                 << "| " << setw(7) << e.joursEscompte
                 << "| " << setw(11) << e.escompte
                 << "| " << setw(11) << e.commission << "|\n";
        }
        cout << "+--------+------------+------------+------------+----------+--------+------------+------------+\n\n";
        double tva = totalAgios * (tauxTVA / 100);
        double totalAvecTVA = totalAgios + tva;
        double netRemis = totalNominal - totalAvecTVA;
        cout << "RECAPITULATIF:\n";
        cout << "----------------------------------------\n";
        cout << left << setw(30) << "Valeur nominale totale:" 
             << fixed << setprecision(3) << totalNominal << " DT\n";
        cout << left << setw(30) << "Total agios HT:" 
             << totalAgios << " DT\n";
        cout << "TVA (" << fixed << setprecision(2) << tauxTVA << "%):";
        cout << setw(30 - 8 - to_string((int)tauxTVA).length()) << "" << tva << " DT\n";
        cout << "----------------------------------------\n";
        cout << left << setw(30) << "Total agios TTC:" 
             << totalAvecTVA << " DT\n";
        cout << "========================================\n";
        cout << left << setw(30) << "NET REMIS AU CLIENT:" 
             << netRemis << " DT\n";
        cout << "========================================\n";
    }
};
int main() {
    cout << "========================================================================\n";
    cout << "              PROGRAMME DE BORDEREAU D'ESCOMPTE BANCAIRE               \n";
    cout << "========================================================================\n\n";
    string dateRemise, lieuBanque, nomBanque;
    double taux, tva;
    double commPD, commPND, commDD, commDND;
    double commFixe, commEndos;
    int nbEscomptes;
    cout << "Date de remise (JJ/MM/AAAA): ";
    getline(cin, dateRemise);
    cout << "Lieu de votre banque (ex: Tunis, Sfax): ";
    getline(cin, lieuBanque);
    cout << "Nom de votre banque (ex: STB, BNA, BIAT): ";
    getline(cin, nomBanque);
    cout << "Taux d'escompte (%): ";
    cin >> taux;
    cout << "Taux TVA (%): ";
    cin >> tva;
    cout << "\n--- COMMISSIONS BANCAIRES ---\n";
    cout << "Commission sur place domicilie (DT): ";
    cin >> commPD;
    cout << "Commission sur place non-domicilie (DT): ";
    cin >> commPND;
    cout << "Commission deplace domicilie (DT): ";
    cin >> commDD;
    cout << "Commission deplace non-domicilie (DT): ";
    cin >> commDND;
    cout << "\nCommission fixe par effet (DT): ";
    cin >> commFixe;
    cout << "Commission d'endos par effet (DT): ";
    cin >> commEndos;
    cout << "\nNombre d'escomptes a traiter: ";
    cin >> nbEscomptes;
    cin.ignore();
    BordereauEscompte bordereau(dateRemise, lieuBanque, nomBanque, taux, tva, 
                                commPD, commPND, commDD, commDND, commFixe, commEndos);
    
    for (int i = 0; i < nbEscomptes; i++) {
        string numero, tireur, dateEcheance, lieu, banque;
        double montant;
        cout << "\n--- Escompte " << (i + 1) << " ---\n";
        cout << "Numero: ";
        getline(cin, numero);
        cout << "Tireur (nom du client): ";
        getline(cin, tireur);
        cout << "Montant: ";
        cin >> montant;
        cin.ignore();
        cout << "Date d'echeance (JJ/MM/AAAA): ";
        getline(cin, dateEcheance);
        cout << "Lieu de paiement (ex: Tunis, Sfax, Gabes): ";
        getline(cin, lieu);
        cout << "Banque domiciliatrice (ex: STB, BNA, BIAT): ";
        getline(cin, banque);
        bordereau.ajouterEscompte(numero, tireur, montant, dateEcheance, lieu, banque);
    }
    bordereau.afficherBordereau();
    return 0;
}
